/// \file ROOT/TDrawable.h
/// \ingroup Base ROOT7
/// \author Axel Naumann <axel@cern.ch>
/// \date 2015-08-07
/// \warning This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback
/// is welcome!

/*************************************************************************
 * Copyright (C) 1995-2015, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT7_TDrawable
#define ROOT7_TDrawable

#include <memory>
#include <string>

namespace ROOT {
namespace Experimental {

class TCanvas;
class TDrawingOptsBase;
class TMenuItems;

namespace Internal {
class TVirtualCanvasPainter;
}

/** \class TDrawable
  Base class for drawable entities: objects that can be painted on a `TPad`.
 */

class TDrawable {
public:
   virtual ~TDrawable();

   virtual void Paint(Internal::TVirtualCanvasPainter &onCanv) = 0;

   /** Method can be used to provide menu items for the drawn object */
   virtual void PopulateMenu(TMenuItems &){};

   virtual void Execute(const std::string &);

   /// Get the reference to the drawing options as TDrawingOptsBase. Used e.g. to identify the TDrawable in
   /// the list of primitives.
   virtual TDrawingOptsBase& GetOptionsBase() = 0;

};

template <class DERIVED>
class TDrawableBase: public TDrawable {
public:
   TDrawingOptsBase& GetOptionsBase() override { return static_cast<DERIVED*>(this)->GetOptions(); }
};

namespace Internal {

/// \class TAnyPtr
/// Models a shared pointer or a unique pointer.

template <class T>
class TUniWeakPtr {
   // Needs I/O support for union (or variant, actually) {
      std::unique_ptr<T> fUnique;
      std::weak_ptr<T> fWeak; //! Cannot save for now :-(
      T* fWeakForIO = nullptr; // Hack to allow streaming *out* of fWeak (reading is still broken because we don't set fWeak)
   // };
   bool fIsWeak = false; ///< fUnique or fWeak?

public:
   /// \class Accessor
   /// Gives transparent access to the shared or unique pointer.
   /// Locks if needed.
   class Accessor {
      union {
         T *fRaw;                    ///< The raw, non-owning pointer accessing a TUniWeak's unique_ptr
         std::shared_ptr<T> fShared; ///< The shared_ptr accessing a TUniWeak's weak_ptr
      };
      bool fIsShared; ///< fRaw or fShared?

   public:
      Accessor(const TUniWeakPtr &uniweak): fIsShared(uniweak.fIsWeak)
      {
         if (fIsShared)
            new (&fShared) std::shared_ptr<T>(uniweak.fWeak.lock());
         else
            fRaw = uniweak.fUnique.get();
      }

      Accessor(Accessor &&rhs): fIsShared(rhs.fIsShared)
      {
         if (fIsShared)
            new (&fShared) std::shared_ptr<T>(std::move(rhs.fShared));
         else
            fRaw = rhs.fRaw;
      }

      T *operator->() const { return fIsShared ? fRaw : fShared.get(); }
      T &operator*() const { return *operator->(); }
      operator bool() const { return fIsShared ? (bool)fRaw : (bool)fShared; }

      ~Accessor()
      {
         if (fIsShared)
            fShared.~shared_ptr();
      }
   };

   TUniWeakPtr() = default;
   TUniWeakPtr(const std::shared_ptr<T> &ptr): fWeak(ptr), fWeakForIO(ptr.get()), fIsWeak(true) {}
   TUniWeakPtr(std::unique_ptr<T> &&ptr): fUnique(std::move(ptr)), fIsWeak(false) {}
   TUniWeakPtr(TUniWeakPtr &&rhs): fIsWeak(rhs.fIsWeak)
   {
      if (rhs.fIsWeak) {
         fWeak = std::move(rhs.fWeak);
         auto shptr = rhs.fWeak.lock();
         fWeakForIO = shptr.get();
      } else {
         fUnique = std::move(rhs.fUnique);
      }
   }

   ~TUniWeakPtr()
   {
   }

   Accessor Get() const { return Accessor(*this); }
   void Reset()
   {
      if (fIsWeak)
         fWeak.reset();
      else
         fUnique.reset();
   }
};

} // namespace Internal
} // namespace Experimental
} // namespace ROOT

#endif
